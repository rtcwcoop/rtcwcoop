FROM alpine:latest as build
RUN apk add build-base sdl2-dev

WORKDIR /src
COPY . .
RUN make

FROM alpine:latest as publish
EXPOSE 27960/UDP
COPY --from=build /src/build/release-linux-x86_64 /home/rtcwcoop
ENTRYPOINT ["/home/rtcwcoop/rtcwcoopded.x86_64"]
