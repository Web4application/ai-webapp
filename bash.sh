make install-system     # System-wide install (/usr/local/bin)
make uninstall-system   # Remove system install
#!/bin/bash
set -e
echo "Packaging kubu-hai-template..."

ZIP_NAME="kubu-hai-template-$(date +%s).zip"
zip -r $ZIP_NAME . -x '*.venv*' '*.pyc' '__pycache__/*' '.git/*'

echo "Packaged as $ZIP_NAME"
