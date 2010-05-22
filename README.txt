; $Id$

********************************************************************
                     D R U P A L    M O D U L E
********************************************************************

Name: Fedora REST API
Author: Don Gourley
Maintainers: 
Drupal: 6


********************************************************************
DESCRIPTION:

This module provides a programming interface to invoke Fedora Commons
REST methods. It includes a PHP class that encapsulates all the REST
API methods so they can be easily invoked from your modules. It also
defines a type of node (fedora_object) that represents a digital
object in the repository. fedora_object can be extended by defining
a new content-type in a custom module and invoking the fedora_rest
hook implementations (particularly fedora_rest_insert and _update) to
add Fedora object attributes to the node.

********************************************************************
INSTALLATION:

1. Place the entire fedora_rest directory into your Drupal
   sites/all/modules directory.

2. Enable the workflow module by navigating to:

     Administer > Site building > Modules > Fedora Commons Repository

   Enabling the Fedora REST API module will create the necessary
   database tables for you.

3. Configure the import parameters at:

     Administer > Site configuration > Fedora API settings


********************************************************************
REPOSITRY REQUIREMENTS:

This module only works with the RESTful Web Service oriented API that
was introduced in Fedora 3, so Fedora Commons 3.0 or greater is
required.

The Fedora REST API module uses the Fedora Commons repository Resource
Index to select objects for importing, so the Resource Index must be
enabled.


********************************************************************
CUSTOM MODULE DEVELOPMENT:

Although you can create a basic web site to publish digital content
from a Fedora Commons repository with just the Fedora REST API
module, it is primarily intended to ease development of more complex
web sites based on a repository. This is done by writing a custom
module that defines a new content-type that inherits from and extends
the fedora_object content-type.

To develop such a custom module, start with the template provided
in module.template. Directions in the header of that file describe
how to replace place-holder strings with the names of your module
and content-types. When you have done that, you have a new type of
node that effectively inherits all behaviors from fedora_object as
the hook implementations in your module simply invoke the fedora_rest
implementations. You can then selectively extend and override those
behaviors by editing the hook implementations.

