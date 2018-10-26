FROM ahmad88me/tadahdt:latest

WORKDIR /app

COPY makefile makefile
COPY src src
RUN mkdir -p build
RUN mkdir -p bin

CMD ["make", "cov"]
