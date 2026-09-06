#!/usr/bin/bash

INSTALL_SCRIPT_FILE_PATH="$(realpath "${BASH_SOURCE[0]}")"
REPO_PATH="$(dirname "$INSTALL_SCRIPT_FILE_PATH")"

# Colour escape codes
BLUE='\033[0;34m'
GREEN='\033[0;32m'
GREY='\033[2;37m'
RED='\033[0;31m'
RESET='\033[0m'

function build_commando() {
    compile_string="g++ ./commando.cpp -o commando"
    echo "$compile_string"

    # shellcheck disable=SC2046
    return $($compile_string)
}

if build_commando; then
    echo -e "${GREEN}Built successfully!${RESET}"
else
    echo -e "${RED}Build failed${RESET}"
    exit 1
fi

BASHRC_PATH=~/.bashrc
#BASHRC_PATH=~/TEST_FILE

if [ ! -f "$BASHRC_PATH" ]; then
    echo "${RED}No .bashrc found!${RESET}"
    exit 1
fi

echo -e -n "${BLUE}Do you want to update $BASHRC_PATH? (y/n):${RESET} "
read choice
# Default to yes if they press enter
choice="${choice:-y}"
case "$choice" in 
    y|Y ) 
        ;;
    n|N ) 
        echo -e "${GREEN}Finished.${RESET}"
        exit 1
        ;;
    * ) 
        echo -e "${RED}Invalid choice.${RESET}"
        exit 1
        ;;
esac

if grep -q "/commando" "$BASHRC_PATH"; then
    echo -e -n "${RED}found 'commando' in '$BASHRC_PATH'.${RESET} ${BLUE}Are you sure you want to continue? (y/n)${RESET}: " 
    read choice
    choice="${choice:-y}"
    case "$choice" in 
        y|Y ) 
            ;;
        n|N ) 
            echo "Finished."
            exit 1
            ;;
        * ) 
            echo "Invalid choice."
            exit 1
            ;;
    esac
fi 

# shellcheck disable=SC2034
COMMANDS_TEXT_FILE_PATH="$REPO_PATH/commands.txt"
# Multiline command string
COMMAND_TO_APPEND_TO_BASHRC=$(
    cat <<EOF

# commando: Print a random script you want to remember on shell start.
$REPO_PATH/commando "$COMMANDS_TEXT_FILE_PATH"
EOF
)

echo "$COMMAND_TO_APPEND_TO_BASHRC" >> "$BASHRC_PATH"
echo -e "${GREEN}$BASHRC_PATH updated${RESET}"


echo "=========================UPDATED WITH=================================="
echo -e -n "${GREY}"
tail -3  $BASHRC_PATH | cat
echo -e -n "${RESET}"
echo "======================================================================="

# shellcheck source=/home/malachy/.bashrc
source "$BASHRC_PATH"