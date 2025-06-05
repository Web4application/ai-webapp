# Stage 1: Build Dart app
FROM dart:3.5 as build

WORKDIR /app

# Copy source
COPY pubspec.* ./
RUN dart pub get

COPY . .

# Build AOT-compiled executable (replace main.dart with your entrypoint)
RUN dart compile exe bin/main.dart -o bin/app_executable

# Stage 2: Minimal runtime image
FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

# Copy the Dart runtime and necessary libraries from Dart official SDK image
COPY --from=dart:3.5 /runtime/ /runtime/

ENV PATH="/runtime/bin:${PATH}"

WORKDIR /app

# Copy the AOT executable from build stage
COPY --from=build /app/bin/app_executable /app/bin/app_executable

# Use a non-root user for security (optional but recommended)
RUN useradd -m dartuser && chown -R dartuser /app
USER dartuser

ENTRYPOINT ["/app/bin/app_executable"]
