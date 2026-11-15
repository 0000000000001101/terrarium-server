FROM debian:trixie-slim AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B /build \
      -DCMAKE_BUILD_TYPE=Release \
    && cmake --build /build --parallel --target TerrariumServer

FROM debian:trixie-slim AS runtime
LABEL org.opencontainers.image.source https://github.com/0000000000001101/terrarium-server

RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=build /build/TerrariumServer ./TerrariumServer

CMD ["./TerrariumServer"]

# FROM gcr.io/distroless/cc-debian13:nonroot AS runtime
# WORKDIR /app
# COPY --from=build /build/TerrariumServer ./TerrariumServer
# ENTRYPOINT ["/app/TerrariumServer"]


# Run this only once when setting things up for the first time:
# `docker buildx create --name terrarium-builder --driver docker-container --bootstrap --use`
# Check it:
# `docker buildx inspect --bootstrap`
# Log in to ghcr.io: (If it asks for a password, it might be wanting a classic GitHub token with `write:packages` scope for the 0000000000001101 org)
# `docker login ghcr.io -u YOUR_GITHUB_USERNAME`
# Build docker container for multiple platforms, and push it to the GHCR: (be sure to increment the version!)
# `docker buildx build --builder terrarium-builder --platform linux/amd64,linux/arm64 -t ghcr.io/0000000000001101/terrarium-server:latest -t ghcr.io/0000000000001101/terrarium-server:0.1.0 --push .`
# Verify:
# `docker buildx imagetools inspect ghcr.io/0000000000001101/terrarium-server:latest`
# NOTE: You may need to go into GitHub and make the package public, as they may be private by default.
