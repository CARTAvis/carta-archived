FROM grimmer0125/cartabuild:20161224

COPY . /home/developer/src/CARTAvis
WORKDIR /home/developer/
USER 1000
# ENV CIRUN=$CIRCLECI
RUN /home/developer/src/CARTAvis/carta/scripts/dockerbuildcarta.sh
RUN ln -s /home/developer/src/CARTAvis/buildindocker /home/developer/src/build
CMD ["bash"]
