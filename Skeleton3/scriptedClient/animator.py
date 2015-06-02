#!/usr/bin/env python
# -*- coding: utf-8 -*-

from cartaview import CartaView

class Animator(CartaView):
    """
    Represents an animator view.
    """

    def setChannel(self, index):
        """
        Display the specified channel of the current image.

        Parameters
        ----------
        index: integer
            The channel number to display.

        Returns
        -------
        list
            Error information if the channel could not be set.
        """
        result = self.con.cmdTagList("setChannel", animatorView=self.getId(), channel=index)
        return result

    def setImage(self, index):
        """
        Display the specified image.

        Parameters
        ----------
        index: integer
            The image number to display.

        Returns
        -------
        list
            Error information if the image could not be set.
        """
        result = self.con.cmdTagList("setImage", animatorView=self.getId(), image=index)
        return result

    def showImageAnimator(self):
        """
        Show the image animator.

        Returns
        -------
        list
            Error information if the image animator could not be shown.
        """
        result = self.con.cmdTagList("showImageAnimator", animatorView=self.getId())
        return result
