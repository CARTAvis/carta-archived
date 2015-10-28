FROM astrilet/cartabuild:latest
ADD . /home/developer/src/CARTAvis
ADD ./carta/scripts/buildcarta.sh /home/developer
ADD ~/images/images.tgz /scratch/Images
WORKDIR /home/developer/
USER 1000
ENV CIRUN $CIRCLECI
RUN ./buildcarta.sh
CMD ["bash"]
