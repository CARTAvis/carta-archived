FROM astrilet/cartabuild:latest
ENV CIRUN $CIRCLECI
COPY . /home/developer/src/CARTAvis
WORKDIR /home/developer/
USER 1000
RUN ./buildcarta.sh
CMD ["bash"]
