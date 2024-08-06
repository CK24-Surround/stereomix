# ====================================================================
# We will be using Unreal Editor's built-in automation tool to include as little third
# party software as possible.
#
# We will be using a Linux container for this exmaple, but please review the specific
# features availability for each contianer type and choose accordingly:
# https://unrealcontainers.com/docs/preliminaries/technology-selection#compatibility-guidance-for-specific-features-and-use-cases
#
# Support for other container types (e.g. Windows containers) is not currently guaranteed.
# ====================================================================


# 1. Create a development image with the tools required to package your server.
FROM ghcr.io/epicgames/unreal-engine:dev-5.4.3 AS builder
COPY --chown=ue4:ue4 . /tmp/project

WORKDIR /tmp/project

# You may want to review the arguments listed below and change them as required by running `RunUAT BuildCookRun -Help`
# where applicable (the RunUAT executable is found in Unreal Editor's installation directory, under Engine/Build/BatchFiles/)
RUN /home/ue4/UnrealEngine/Engine/Build/BatchFiles/RunUAT.sh BuildCookRun \
	-serverplatform=Linux \
	-project=/tmp/project/StereoMix.uproject \
	-serverconfig=Shipping \
	-noP4 \
	-cook \
	-allmaps \
	-build \
	-stage \
	-prereqs \
	-pak \
	-archive \
	-archivedirectory=/tmp/project/dist \
	-server \
	-noclient

# 2. Create a Runtime image from the packaged server.
FROM ghcr.io/epicgames/unreal-engine:runtime
COPY --from=builder --chown=ue4:ue4 /tmp/project/dist/LinuxServer /home/ue4/project

EXPOSE 7777/udp

ENTRYPOINT ["/home/ue4/project/StereoMixServer.sh", "-log"]