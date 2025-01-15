param (
    [Parameter(Mandatory=$false)]
    [Switch] $clean
)

& ./make-autohooks.ps1

function Clean-Build-Folder {
    if (Test-Path -Path "build")
    {
        remove-item build -R
        new-item -Path build -ItemType Directory
    } else {
        new-item -Path build -ItemType Directory
    }
}

if ($clean.IsPresent) {
    Clean-Build-Folder
}

# build mod
& cmake -G "Ninja" -DCMAKE_BUILD_TYPE="RelWithDebInfo" . -B build
& cmake --build ./build

$ExitCode = $LastExitCode

exit $ExitCode
