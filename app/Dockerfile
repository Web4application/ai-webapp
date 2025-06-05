# Use an official Python runtime as a parent image
FROM python:3.8-slim

# Set the working directory
WORKDIR /app

# Install dependencies
COPY requirements.txt ./
RUN pip install --no-cache-dir -r requirements.txt

# Copy the rest of the application
COPY . .

# Set environment variable for Flask
ENV FLASK_APP=app.py

# Expose the port Flask runs on
EXPOSE 5000

# Run the application
CMD ["flask", "run", "--host=0.0.0.0"]
