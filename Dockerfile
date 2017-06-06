FROM carta/cartabuild:base_20170603

COPY . /home/developer/src/CARTAvis
WORKDIR /home/developer/
USER 1000
# ENV CIRUN=$CIRCLECI
RUN /home/developer/src/CARTAvis/carta/scripts/dockerbuildcarta.sh

# some scripts still use /home/developer/src/build
RUN ln -s /home/developer/src/CARTAvis/buildindocker /home/developer/src/build
CMD ["bash"]
