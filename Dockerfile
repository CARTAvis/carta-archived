FROM vsuorant/cartabuild:latest
COPY . /home/developer/src/CARTAvis
WORKDIR /home/developer/
USER 1000
ENV CIRUN $CIRCLECI
RUN /home/developer/src/CARTAvis/carta/scripts/buildcarta.sh
CMD ["bash"]
