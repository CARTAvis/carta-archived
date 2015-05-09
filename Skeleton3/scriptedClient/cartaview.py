#!/usr/bin/env python
# -*- coding: utf-8 -*-

class CartaView:
    """Base class for Carta objects"""
    def __init__(self, idStr, connection):
        self.__id = idStr
        self.con = connection
        return

    def getId(self):
        """This is mainly for testing/debugging/sanity purposes"""
        return self.__id

    def addLink(self, imageView):
        result = self.con.cmdTagList("addLink", sourceView=self.getId(), destView=imageView.getId())
        return result

    def removeLink(self, imageView):
        result = self.con.cmdTagList("removeLink", sourceView=self.getId(), destView=imageView.getId())
        return result
