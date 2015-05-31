#!/usr/bin/env python
# -*- coding: utf-8 -*-

from cartaview import CartaView

class Animator(CartaView):
    """Represents an animator view"""

    def setChannel(self, index):
        result = self.con.cmdTagList("setChannel", animatorView=self.getId(), channel=index)
        return result

    def setImage(self, index):
        result = self.con.cmdTagList("setImage", animatorView=self.getId(), image=index)
        return result

    def showImageAnimator(self):
        result = self.con.cmdTagList("showImageAnimator", animatorView=self.getId())
        return result
