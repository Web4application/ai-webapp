import tensorflow as tf
from tensorflow.keras import layers, models

# Load and preprocess your dataset
# Assume X_train, y_train, X_test, y_test are your data and labels

model = models.Sequential([
layers.Conv2D(32, (3, 3), activation='relu', input_shape=(input_shape)),
layers.MaxPooling2D((2, 2)),
layers.Conv2D(64, (3, 3), activation='relu'),
layers.MaxPooling2D((2, 2)),
layers.Conv2D(64, (3, 3), activation='relu'),
layers.Flatten(),
layers.Dense(64, activation='relu'),
layers.Dense(1, activation='sigmoid')
])

model.compile(optimizer='adam', loss='binary_crossentropy', metrics=['accuracy'])
model.fit(X_train, y_train, epochs=10, validation_data=(X_test, y_test))
model.save('wake_word_model.h5')

Step 2: Real-Time Wake Word Detection
Use the trained model to detect the wake word in real-time.

import numpy as np
import pyaudio
import librosa

# Load the trained model
model = tf.keras.models.load_model('wake_word_model.h5')
