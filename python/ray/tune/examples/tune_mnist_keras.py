import argparse
import os
import sys

from filelock import FileLock

import ray
from ray import tune
from ray.tune.schedulers import AsyncHyperBandScheduler

if sys.version_info >= (3, 12):
    # Tensorflow is not installed for Python 3.12 because of keras compatibility.
    sys.exit(0)
else:
    from tensorflow.keras.datasets import mnist

    from ray.air.integrations.keras import ReportCheckpointCallback


def train_mnist(config):
    # https://github.com/tensorflow/tensorflow/issues/32159
    import tensorflow as tf

    batch_size = 128
    num_classes = 10
    epochs = 12

    with FileLock(os.path.expanduser("~/.data.lock")):
        (x_train, y_train), (x_test, y_test) = mnist.load_data()
    x_train, x_test = x_train / 255.0, x_test / 255.0
    model = tf.keras.models.Sequential(
        [
            tf.keras.layers.Flatten(input_shape=(28, 28)),
            tf.keras.layers.Dense(config["hidden"], activation="relu"),
            tf.keras.layers.Dropout(0.2),
            tf.keras.layers.Dense(num_classes, activation="softmax"),
        ]
    )

    model.compile(
        loss="sparse_categorical_crossentropy",
        optimizer=tf.keras.optimizers.SGD(lr=config["lr"], momentum=config["momentum"]),
        metrics=["accuracy"],
    )

    model.fit(
        x_train,
        y_train,
        batch_size=batch_size,
        epochs=epochs,
        verbose=0,
        validation_data=(x_test, y_test),
        callbacks=[
            ReportCheckpointCallback(
                checkpoint_on=[], metrics={"mean_accuracy": "accuracy"}
            )
        ],
    )


def tune_mnist(num_training_iterations):
    sched = AsyncHyperBandScheduler(
        time_attr="training_iteration", max_t=400, grace_period=20
    )

    tuner = tune.Tuner(
        tune.with_resources(train_mnist, resources={"cpu": 2, "gpu": 0}),
        run_config=tune.RunConfig(
            name="exp",
            stop={"mean_accuracy": 0.99, "training_iteration": num_training_iterations},
        ),
        tune_config=tune.TuneConfig(
            scheduler=sched,
            metric="mean_accuracy",
            mode="max",
            num_samples=10,
        ),
        param_space={
            "threads": 2,
            "lr": tune.uniform(0.001, 0.1),
            "momentum": tune.uniform(0.1, 0.9),
            "hidden": tune.randint(32, 512),
        },
    )
    results = tuner.fit()
    print("Best hyperparameters found were: ", results.get_best_result().config)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--smoke-test", action="store_true", help="Finish quickly for testing"
    )
    args, _ = parser.parse_known_args()

    if args.smoke_test:
        ray.init(num_cpus=4)

    tune_mnist(num_training_iterations=2 if args.smoke_test else 300)
