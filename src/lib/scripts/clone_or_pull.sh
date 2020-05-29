#!/bin/bash
GIT_DIR="$1"
URL="$2"
if [ ! -d "$GIT_DIR" ]; then # if folder "GIT_DIR" does not exists
  git clone "$URL" "$GIT_DIR"
else
  cd "$GIT_DIR"
  if [[ $(git pull) = "Already up to date." ]]; then
    # we can skip recompiling, because the last build is already up to date
    echo "$GIT_DIR already up to date."
    exit 0
  fi
fi
exit 1