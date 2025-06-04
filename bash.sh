make install-system     # System-wide install (/usr/local/bin)
make uninstall-system   # Remove system install
#!/bin/bash
set -e
echo "Packaging kubu-hai-template..."

ZIP_NAME="kubu-hai-template-$(date +%s).zip"
zip -r $ZIP_NAME . -x '*.venv*' '*.pyc' '__pycache__/*' '.git/*'

echo "Packaged as $ZIP_NAME"

git clone https://github.com/Web4application/kubu-hai-template.git
cd kubu-hai-template

# Copy or move your existing files into this directory
cp -r /path/to/your/project/* .

git add .
git commit -m "Initial upload of kubu-hai-template"
git push origin main
