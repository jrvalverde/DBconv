dbconv.exe : dbconv.obj dbsetup.obj dbproc.obj

	$ link dbconv,dbsetup,dbproc,'rtlib'

	$ purge



dbconv.obj : dbconv.c dbconv.h

	$ cc dbconv



dbsetup.obj : dbsetup.c dbconv.h

	$ cc dbsetup



dbproc.obj : dbproc.c dbconv.h

	$ cc dbproc

