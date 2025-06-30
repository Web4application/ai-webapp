npm install -g @angular/cli

Read more here: https://locall.host/index-php/

ng new my-app

Read more here: https://locall.host/index-php/

cd my-app ng serve

Read more here: https://locall.host/index-php/


make install-system     # System-wide install (/usr/local/bin)
make uninstall-system   # Remove system install
#!/bin/bash
set -e
echo "Packaging ai-webapp..."

ZIP_NAME="ai-webapp-$(date +%s).zip"
zip -r $ZIP_NAME . -x '*.venv*' '*.pyc' '__pycache__/*' '.git/*'

echo "Packaged as $ZIP_NAME"

git clone https://github.com/Web4application/kubu-hai-template.git
cd ai-webapp

# Copy or move your existing files into this directory
cp -r /path/to/your/project/* .

git add .
git commit -m "Initial upload of kubu-hai-template"
git push origin main

curl -SL https://github.com/docker/compose/releases/download/v2.37.3/docker-compose-linux-x86_64 -o /usr/local/bin/docker-compose

 Start-BitsTransfer -Source "https://github.com/docker/compose/releases/download/v2.37.3/docker-compose-windows-x86_64.exe" -Destination $Env:ProgramFiles\Docker\docker-compose.exe

 composer create-project hunwalk/yii2-basic-firestarter <project_pilot_AI> --prefer-dist

 php yii migrate-user
 php yii migrate-rbac
 php yii migrate

 cp .env.example .env

npm install --save-dev @commitlint/config-conventional @commitlint/cli
echo "export default {extends: ['@commitlint/config-conventional']};" > commitlint.config.js

npm install commitizen -g

npm
commitizen init cz-conventional-changelog --save-dev --save-exact

# yarn
commitizen init cz-conventional-changelog --yarn --dev --exact

# pnpm
commitizen init cz-conventional-changelog --pnpm --save-dev --save-exact

npx commitizen init cz-conventional-changelog --save-dev --save-exact
