echo "update linker caches..."
ldconfig

echo "[user] 
    name = $GITNAME 
    email = $GITEMAIL 
    " > ~/.gitconfig
make codecov
