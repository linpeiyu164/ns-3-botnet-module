build:
	mkdir -p /home/${USER}/ns3-botnet-module && cd /home/${USER}/ns3-botnet-module
	docker build -t ns3-image .
run:
	docker run -it --rm --name ns3-botnet-env \
	-v "/workspace:/home/${USER}/ns3-botnet-module" ns3-image