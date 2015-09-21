FROM astrilet/cartabuild:latest
COPY . /home/developer/src/CARTAvis
WORKDIR /home/developer/
USER 1000
ENV CIRUN $CIRCLECI
RUN ./buildcarta.sh
CMD ["bash"]
