This is an attempt to have a more centralized place to configure
library locations on a developer machine.

Just change the symbolic links here to point to where your libraries are.

I am thinking of adding a script that will automatically create these links
based on some environment variables. That way when you check out the sources, you run this script and the links are auto-generated. Maybe we can then remove the actual links from git...

