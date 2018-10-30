echo "[user] 
    name = $GITNAME 
    email = $GITEMAIL 
    " > ~/.gitconfig
make cov
make codecov
#make clean
