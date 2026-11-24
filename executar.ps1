param(
    [switch]$Testar
)

$ErrorActionPreference = 'Stop'
Set-Location -LiteralPath $PSScriptRoot

if (-not (Get-Command gcc -ErrorAction SilentlyContinue)) {
    Write-Host 'GCC nao encontrado. Instale o MinGW-w64 e adicione a pasta bin ao PATH.' -ForegroundColor Red
    exit 1
}

$argumentos = @('-Wall', '-Wextra', '-std=c99', '-I.', '-Ibackend', '-Itelas')
if ($Testar) {
    $fontes = @('tests/test_regras.c', 'backend/backend.c', 'telas/telas.c')
    $destino = 'obj/test_regras.exe'
    New-Item -ItemType Directory -Force -Path 'obj' | Out-Null
} else {
    $fontes = @('main.c', 'backend/backend.c', 'telas/telas.c')
    $destino = 'sistema_recanto.exe'
}

Write-Host 'Compilando o projeto...'
& gcc @argumentos @fontes -o $destino -lbcrypt
if ($LASTEXITCODE -ne 0) {
    Write-Host 'Nao foi possivel compilar o projeto.' -ForegroundColor Red
    exit $LASTEXITCODE
}

if ($Testar) {
    & .\obj\test_regras.exe
} else {
    & .\sistema_recanto.exe
}
exit $LASTEXITCODE
