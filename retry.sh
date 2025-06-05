ANSI_RED="\033[31;1m"
ANSI_GREEN="\033[32;1m"
ANSI_YELLOW="\033[33;1m"
ANSI_RESET="\033[0m"
ANSI_CLEAR="\033[0K"

# Number of attempts.
RETRY_COUNT=2

travis_retry() {
  local result=0
  local count=1
  while [ $count -le $RETRY_COUNT ]; do
    [ $result -ne 0 ] && {
      echo -e "\n${ANSI_RED}The command \"$@\" failed. Retrying, $count of ${RETRY_COUNT}.${ANSI_RESET}\n" >&2
    }
    "$@" && { result=0 && break; } || result=$?
    count=$(($count + 1))
    sleep 1
  done

  [ $count -gt $RETRY_COUNT ] && {
    echo -e "\n${ANSI_RED}The command \"$@\" failed ${RETRY_COUNT} times.${ANSI_RESET}\n" >&2
  }

  return $result
}

travis_retry "$@"
