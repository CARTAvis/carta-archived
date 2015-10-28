FROM astrilet/cartabuild:latest
COPY . /home/developer/src/CARTAvis
COPY ./carta/scripts/buildcarta.sh /home/developer
COPY ./carta/scripts/runScriptedClientTests.sh  /home/developer
COPY ./carta/scripts/startCARTAServer.sh /home/developer
WORKDIR /home/developer/
USER 1000
ENV CIRUN $CIRCLECI
RUN ./buildcarta.sh
CMD ["bash"]
