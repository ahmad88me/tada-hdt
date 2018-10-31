FROM ahmad88me/tadahdt:latest

WORKDIR /app

COPY makefile makefile
COPY src src
#For .git is required for the coverage
COPY .git .git
COPY *.sh /app/

RUN mkdir -p build
RUN mkdir -p bin
ENV CODECOV_TOKEN "5a8fc669-b5c4-4982-acda-5821ab6f7379"
CMD ["sh", "start.sh"]
#CMD ["sh", "semaphoreci.sh"]
