#!/usr/bin/env python
# -*- coding: utf-8 -*-

class CartaView:
    """
    Base class for Carta GUI objects.

    Parameters
    ----------
    idStr: string
       The unique server-side ID of a Carta object. 
    connection: TagConnector object
        Provides facilities for communicating with C++.
    """

    def __init__(self, idStr, connection):
        self.__id = idStr
        self.con = connection
        return

    def getId(self):
        """
        Get the ID value of a Carta object.

        Returns
        -------
        string
            The unique server-side ID of a Carta object.
        """
        return self.__id

    def addLink(self, imageView):
        """
        Establish a link between a source and an image view.
        The parameter is an Image object that can be obtained with the
        getImageViews() command. For example, the following sequence of
        commands will obtain the image views and histogram views, then
        add a link between the first histogram view and the first image
        view:

            i = v.getImageViews()
            h = v.getHistogramViews()
            h[0].addLink(i[0])

        Parameters
        ----------
        dest: Image object
            The destination object for the link.

        Returns
        -------
        list
            An error message if there was a problem adding the link;
            empty otherwise.
        """
        result = self.con.cmdTagList("addLink", sourceView=self.getId(),
                                     destView=imageView.getId())
        return result

    def removeLink(self, imageView):
        """
        Remove a link between a source and an image view.
        The parameter is an Image object that can be obtained with the
        getImageViews() command. For example, the following sequence of
        commands will obtain the image views and histogram views, then
        remove the link between the first histogram view and the first
        image view:

            i = v.getImageViews()
            h = v.getHistogramViews()
            h[0].removeLink(i[0])

        Parameters
        ----------
        dest: Image object
            The destination object for the link.

        Returns
        -------
        list
            An error message if there was a problem removing the link;
            empty otherwise.
        """
        result = self.con.cmdTagList("removeLink", sourceView=self.getId(),
                                     destView=imageView.getId())
        return result
