FROM astrilet/carabuild:latest
COPY . /home/developer/src/CARTAvis
WORKDIR /home/developer/
RUN ./buildcarta.sh
CMD ["bash"]
