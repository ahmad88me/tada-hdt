docker image build -t tadahdtdev:latest .
docker container run -e GITEMAIL="`git config user.email`" -e GITNAME="`git config user.name`" --interactive --tty --rm --name tadahdtdev tadahdtdev:latest
