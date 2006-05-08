# Copyright 1999-2006 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header$

inherit subversion

ESVN_REPO_URI="http://svn.nomi.cz/svn/tomi/xchat-bttrw/head"
ESVN_PATCHES="*.diff"
ESVN_OPTIONS="--ignore-externals"

DESCRIPTION="IRC gateway for czech chatting portal Xchat.cz"
HOMEPAGE="http://www.nomi.cz/projects.shtml?id=xchat-bttrw"
SRC_URI=""

LICENSE="GPL-2"
KEYWORDS="~x86"
SLOT="0"

DEPEND="${RDEPEND}
    TomiTCP-svn"
RDEPEND=""

src_compile() {
	SVN_DIR="${PORTDIR}/distfiles/svn-src/" emake || die
}

src_install() {
	dodoc COPYRIGHT COPYING README
	newbin gate xchat-bttrw

        insinto /usr/share/xchat-bttrw
        doins COPYRIGHT

	insinto /etc/conf.d
	newins ${FILESDIR}/xchat-bttrw.confd xchat-bttrw

	exeinto /etc/init.d
	doexe ${FILESDIR}/xchat-bttrw
}

# $Id$
