#new player models

# Introduction #

You need to generate a movespeed file to make your model work


# Details #
A dedicated server doesn't has access to the renderer functions, so it fails
when it tries to retrieve the movespeeds for the CASTAI's.

To solve this problem we write out the movespeeds (once) to a file, so we
can introdruce them to our pk3 files.

When the server can't find the a modelname.mvspd file it requests the local client
to generate such a file.