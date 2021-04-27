FROM debian:buster-slim AS builder
RUN apt-get update --yes && apt-get install make gcc musl musl-tools --yes

WORKDIR /usr/src/keysmash
COPY . .
RUN make release

FROM scratch
WORKDIR /
COPY --from=builder /usr/src/keysmash/server /server
ENTRYPOINT ["/server"]