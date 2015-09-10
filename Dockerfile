FROM astrilet/cartabuild:latest
COPY . /home/developer/src/CARTAvis
WORKDIR /home/developer/
RUN ./buildcarta.sh
CMD ["bash"]
