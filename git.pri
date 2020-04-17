
# Need to discard STDERR so get path to NULL device
win32 {
    NULL_DEVICE = NUL # Windows doesn't have /dev/null but has NUL
} else {
    NULL_DEVICE = /dev/null
}

# Need to call git with manually specified paths to repository
win32 {
BASE_GIT_COMMAND = git
} else {
BASE_GIT_COMMAND = git --git-dir $$PWD/.git --work-tree $$PWD
}

GIT_SHA = $$system($$BASE_GIT_COMMAND rev-parse --short HEAD 2> $$NULL_DEVICE)

GIT_BRANCH = $$system($$BASE_GIT_COMMAND rev-parse --abbrev-ref HEAD 2> $$NULL_DEVICE)

DEFINES += GIT_SHA=\\\"$$GIT_SHA\\\"
DEFINES += GIT_BRANCH=\\\"$$GIT_BRANCH\\\"
