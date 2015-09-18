FROM astrilet/cartabuild:latest
ENV CIRUN $CIRCLECI
COPY . /home/developer/src/CARTAvis
WORKDIR /home/developer/
RUN ./buildcarta.sh
CMD ["bash"]
