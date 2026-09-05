FROM fedora:42

RUN dnf -y install gcc make libcmocka libcmocka-devel \
    && dnf clean all

WORKDIR /workspace

CMD ["bash"]
