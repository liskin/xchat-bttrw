# Copyright 1999-2006 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header$

inherit subversion

ESVN_REPO_URI="http://svn.nomi.cz/svn/TomiTCP/head"

DESCRIPTION="Multifunctional C++ networking library"
HOMEPAGE="http://www.nomi.cz/projects.shtml?id=TomiTCP"
SRC_URI=""

LICENSE="GPL-2"
KEYWORDS=""
SLOT="0"

DEPEND="${RDEPEND}"
RDEPEND=""

src_compile() {
	emake || die
}

src_install() {
        dolib libTomiTCP.a
        insinto /usr/include/TomiTCP
        doins *.h
	dodoc COPYRIGHT COPYING

}

# $Id$
