##
## This file is part of Gambit
## Copyright (c) 1994-2014, The Gambit Project (http://www.gambit-project.org)
##
## FILE: CMakeLists.txt
## Top-level CMake file for Gambit
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
##

cmake_minimum_required(VERSION 2.8.12)

project(Gambit)

set(Gambit_VERSION 14.0.2)

option(Gambit_DISABLE_GUI "Don't build graphical interface" OFF)
option(Gambit_DISABLE_ENUMPOLY "Don't build gambit-enumpoly (not supported on 64bit)" OFF)

configure_file(
	"${PROJECT_SOURCE_DIR}/config.cmake.h.in"
	"${PROJECT_BINARY_DIR}/config.h"
)

include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/src/labenski/include
	${PROJECT_BINARY_DIR}
)

set(libagg_la_SOURCES
	src/libgambit/gameagg.cc
	src/libgambit/gamebagg.cc
	src/libagg/agg.cc
	src/libagg/bagg.cc
)

set(libagginclude_HEADERS
	src/libgambit/gameagg.h
	src/libgambit/gamebagg.h
	src/libagg/GrayComposition.h
	src/libagg/agg.h
	src/libagg/bagg.h
	src/libagg/proj_func.h
	src/libagg/trie_map.h
	src/libagg/trie_map.imp
)

set(libgambit_la_SOURCES
	src/libgambit/integer.cc
	src/libgambit/rational.cc
	src/libgambit/shared_ptr.h
	src/libgambit/vector.cc
	src/libgambit/pvector.cc
	src/libgambit/dvector.cc
	src/libgambit/matrix.cc
	src/libgambit/sqmatrix.cc
	src/libgambit/game.cc
	src/libgambit/gameexpl.h
	src/libgambit/gametable.cc
	src/libgambit/gametable.h
	src/libgambit/gametree.cc
	src/libgambit/gametree.h
	src/libgambit/behav.cc
	src/libgambit/behavitr.cc
	src/libgambit/behavspt.cc
	src/libgambit/mixed.cc
	src/libgambit/stratitr.cc
	src/libgambit/stratspt.cc
	src/libgambit/nash.cc
	src/libgambit/nash.h
	src/libgambit/file.cc
	${libagg_la_SOURCES}
)

set(libgambitinclude_HEADERS
	src/libgambit/integer.h
	src/libgambit/rational.h
	src/libgambit/array.h
	src/libgambit/list.h
	src/libgambit/vector.h
	src/libgambit/vector.imp
	src/libgambit/pvector.h
	src/libgambit/pvector.imp
	src/libgambit/dvector.h
	src/libgambit/dvector.imp
	src/libgambit/recarray.h
	src/libgambit/matrix.h
	src/libgambit/matrix.imp
	src/libgambit/sqmatrix.h
	src/libgambit/sqmatrix.imp
	src/libgambit/number.h
	src/libgambit/game.h
	src/libgambit/behav.h
	src/libgambit/behav.imp
	src/libgambit/behavitr.h
	src/libgambit/behavspt.h
	src/libgambit/mixed.h
	src/libgambit/mixed.imp
	src/libgambit/stratitr.h
	src/libgambit/stratspt.h
	src/libgambit/libgambit.h
	${libagginclude_HEADERS}
)

add_library(libgambit ${libgambit_la_SOURCES} ${libgambitinclude_HEADERS})

set(gambit_nfg2html_SOURCES
	src/tools/convert/nfg2html.cc
)

add_executable(gambit-nfg2html ${gambit_nfg2html_SOURCES})
target_link_libraries(gambit-nfg2html libgambit)

set(gambit_nfg2tex_SOURCES
	src/tools/convert/nfg2tex.cc
)

add_executable(gambit-nfg2tex ${gambit_nfg2tex_SOURCES})
target_link_libraries(gambit-nfg2tex libgambit)

set(liblinear_la_SOURCES
	src/liblinear/basis.cc
	src/liblinear/basis.h
	src/liblinear/bfs.h
	src/liblinear/btableau.cc
	src/liblinear/btableau.h
	src/liblinear/btableau.imp
	src/liblinear/lpsolve.cc
	src/liblinear/lpsolve.h
	src/liblinear/lpsolve.imp
	src/liblinear/lptab.cc
	src/liblinear/lptab.h
	src/liblinear/lptab.imp
	src/liblinear/ludecomp.cc
	src/liblinear/ludecomp.h
	src/liblinear/ludecomp.imp
	src/liblinear/tableau.h
	src/liblinear/tableau.cc
)

add_library(liblinear ${liblinear_la_SOURCES})

set(gambit_enummixed_SOURCES
	src/tools/enummixed/lrsmp.h
	src/tools/enummixed/lrsmp.c
	src/tools/enummixed/lrslib.h
	src/tools/enummixed/lrslib.c
	src/tools/enummixed/lrsnash.cc
	src/tools/enummixed/vertenum.h
	src/tools/enummixed/vertenum.imp
	src/tools/enummixed/clique.cc
	src/tools/enummixed/clique.h
	src/tools/enummixed/enummixed.cc
)

add_executable(gambit-enummixed ${gambit_enummixed_SOURCES})
target_link_libraries(gambit-enummixed libgambit liblinear)

if(NOT Gambit_DISABLE_ENUMPOLY)
	# For enumpoly, sources starting in 'pel' are from Pelican;
	# sources from gpartltr to quiksolv were formerly in convenience lib libpoly.
	set(gambit_enumpoly_SOURCES
		src/tools/enumpoly/pelclhpk.cc
		src/tools/enumpoly/pelclhpk.h
		src/tools/enumpoly/pelclqhl.cc
		src/tools/enumpoly/pelclqhl.h
		src/tools/enumpoly/pelclyal.cc
		src/tools/enumpoly/pelclyal.h
		src/tools/enumpoly/pelconv.cc
		src/tools/enumpoly/pelconv.h
		src/tools/enumpoly/peleval.cc
		src/tools/enumpoly/peleval.h
		src/tools/enumpoly/pelgennd.cc
		src/tools/enumpoly/pelgennd.h
		src/tools/enumpoly/pelgmatr.cc
		src/tools/enumpoly/pelgmatr.h
		src/tools/enumpoly/pelgntyp.h
		src/tools/enumpoly/pelhomot.cc
		src/tools/enumpoly/pelhomot.h
		src/tools/enumpoly/pelpred.cc
		src/tools/enumpoly/pelpred.h
		src/tools/enumpoly/pelprgen.cc
		src/tools/enumpoly/pelprgen.h
		src/tools/enumpoly/pelproc.cc
		src/tools/enumpoly/pelproc.h
		src/tools/enumpoly/pelpscon.h
		src/tools/enumpoly/pelpsys.cc
		src/tools/enumpoly/pelpsys.h
		src/tools/enumpoly/pelqhull.cc
		src/tools/enumpoly/pelqhull.h
		src/tools/enumpoly/pelsymbl.cc
		src/tools/enumpoly/pelsymbl.h
		src/tools/enumpoly/pelutils.cc
		src/tools/enumpoly/pelutils.h
		src/tools/enumpoly/gpartltr.cc
		src/tools/enumpoly/gpartltr.h
		src/tools/enumpoly/gpartltr.imp
		src/tools/enumpoly/gpoly.cc
		src/tools/enumpoly/gpoly.h
		src/tools/enumpoly/gpoly.imp
		src/tools/enumpoly/gpolyctr.h
		src/tools/enumpoly/gpolylst.cc
		src/tools/enumpoly/gpolylst.h
		src/tools/enumpoly/gpolylst.imp
		src/tools/enumpoly/gsolver.cc
		src/tools/enumpoly/gsolver.h
		src/tools/enumpoly/gsolver.imp
		src/tools/enumpoly/ideal.cc
		src/tools/enumpoly/ideal.h
		src/tools/enumpoly/ideal.imp
		src/tools/enumpoly/ineqsolv.cc
		src/tools/enumpoly/ineqsolv.h
		src/tools/enumpoly/ineqsolv.imp
		src/tools/enumpoly/interval.cc
		src/tools/enumpoly/interval.h
		src/tools/enumpoly/interval.imp
		src/tools/enumpoly/monomial.cc
		src/tools/enumpoly/monomial.h
		src/tools/enumpoly/monomial.imp
		src/tools/enumpoly/pelclass.cc
		src/tools/enumpoly/pelclass.h
		src/tools/enumpoly/poly.cc
		src/tools/enumpoly/poly.h
		src/tools/enumpoly/poly.imp
		src/tools/enumpoly/prepoly.cc
		src/tools/enumpoly/prepoly.h
		src/tools/enumpoly/quiksolv.cc
		src/tools/enumpoly/quiksolv.h
		src/tools/enumpoly/quiksolv.imp
		src/tools/enumpoly/rectangl.cc
		src/tools/enumpoly/rectangl.h
		src/tools/enumpoly/rectangl.imp
		src/tools/enumpoly/behavextend.cc
		src/tools/enumpoly/behavextend.h
		src/tools/enumpoly/complex.cc
		src/tools/enumpoly/complex.h
		src/tools/enumpoly/gtree.h
		src/tools/enumpoly/gtree.imp
		src/tools/enumpoly/linrcomb.cc
		src/tools/enumpoly/linrcomb.h
		src/tools/enumpoly/linrcomb.imp
		src/tools/enumpoly/efgensup.cc
		src/tools/enumpoly/efgensup.h
		src/tools/enumpoly/gnarray.h
		src/tools/enumpoly/gnarray.imp
		src/tools/enumpoly/sfg.cc
		src/tools/enumpoly/sfg.h
		src/tools/enumpoly/sfstrat.cc
		src/tools/enumpoly/sfstrat.h
		src/tools/enumpoly/nfgensup.cc
		src/tools/enumpoly/nfgensup.h
		src/tools/enumpoly/odometer.cc
		src/tools/enumpoly/odometer.h
		src/tools/enumpoly/nfgcpoly.cc
		src/tools/enumpoly/nfgcpoly.h
		src/tools/enumpoly/nfghs.cc
		src/tools/enumpoly/nfghs.h
		src/tools/enumpoly/efgpoly.cc
		src/tools/enumpoly/nfgpoly.cc
		src/tools/enumpoly/enumpoly.cc
	)

	add_executable(gambit-enumpoly ${gambit_enumpoly_SOURCES})
	target_link_libraries(gambit-enumpoly libgambit)
endif()

set(gambit_enumpure_SOURCES
	src/tools/enumpure/enumpure.cc
)

add_executable(gambit-enumpure ${gambit_enumpure_SOURCES})
target_link_libraries(gambit-enumpure libgambit)

set(gambit_gnm_SOURCES
	src/tools/gt/cmatrix.cc
	src/tools/gt/cmatrix.h
	src/tools/gt/gnm.cc
	src/tools/gt/gnm.h
	src/tools/gt/gnmgame.cc
	src/tools/gt/gnmgame.h
	src/tools/gt/nfgame.cc
	src/tools/gt/nfgame.h
	src/tools/gt/aggame.cc
	src/tools/gt/aggame.h
	src/tools/gt/nfggnm.cc
)

add_executable(gambit-gnm ${gambit_gnm_SOURCES})
target_link_libraries(gambit-gnm libgambit)

set(gambit_ipa_SOURCES
	src/tools/gt/cmatrix.cc
	src/tools/gt/cmatrix.h
	src/tools/gt/gnmgame.cc
	src/tools/gt/gnmgame.h
	src/tools/gt/ipa.cc
	src/tools/gt/ipa.h
	src/tools/gt/nfgame.cc
	src/tools/gt/nfgame.h
	src/tools/gt/aggame.cc
	src/tools/gt/aggame.h
	src/tools/gt/nfgipa.cc
)

add_executable(gambit-ipa ${gambit_ipa_SOURCES})
target_link_libraries(gambit-ipa libgambit)

set(gambit_lcp_SOURCES
	src/tools/lcp/lemketab.cc
	src/tools/lcp/lemketab.h
	src/tools/lcp/lemketab.imp
	src/tools/lcp/lhtab.cc
	src/tools/lcp/lhtab.h
	src/tools/lcp/lhtab.imp
	src/tools/lcp/efglcp.cc
	src/tools/lcp/efglcp.h
	src/tools/lcp/nfglcp.cc
	src/tools/lcp/nfglcp.h
	src/tools/lcp/lcp.cc
)

add_executable(gambit-lcp ${gambit_lcp_SOURCES})
target_link_libraries(gambit-lcp libgambit liblinear)

set(gambit_liap_SOURCES
	src/tools/liap/funcmin.cc
	src/tools/liap/funcmin.h
	src/tools/liap/efgliap.cc
	src/tools/liap/nfgliap.cc
	src/tools/liap/liap.cc
)

add_executable(gambit-liap ${gambit_liap_SOURCES})
target_link_libraries(gambit-liap libgambit)

set(gambit_logit_SOURCES
	src/tools/logit/logbehav.h
	src/tools/logit/logbehav.imp
	src/tools/logit/path.cc
	src/tools/logit/path.h
	src/tools/logit/efglogit.h
	src/tools/logit/efglogit.cc
	src/tools/logit/nfglogit.h
	src/tools/logit/nfglogit.cc
	src/tools/logit/logit.cc
)

add_executable(gambit-logit ${gambit_logit_SOURCES})
target_link_libraries(gambit-logit libgambit)

set(gambit_nfg_logitdyn_SOURCES
	src/tools/logit/nfgdyn.cc
)

add_executable(gambit-nfg-logitdyn ${gambit_nfg_logitdyn_SOURCES})
target_link_libraries(gambit-nfg-logitdyn libgambit)

set(gambit_lp_SOURCES
	src/tools/lp/efglp.cc
	src/tools/lp/efglp.h
	src/tools/lp/nfglp.cc
	src/tools/lp/nfglp.h
	src/tools/lp/lp.cc
)

add_executable(gambit-lp ${gambit_lp_SOURCES})
target_link_libraries(gambit-lp libgambit liblinear)

set(gambit_simpdiv_SOURCES
	src/tools/simpdiv/nfgsimpdiv.cc
)

add_executable(gambit-simpdiv ${gambit_simpdiv_SOURCES})
target_link_libraries(gambit-simpdiv libgambit)

set(gambit_testagg_SOURCES
	src/libagg/getpayoffs.cc
)

add_executable(gambit-testagg ${gambit_testagg_SOURCES})
target_link_libraries(gambit-testagg libgambit)

set(gambit_SOURCES
	src/labenski/src/sheetatr.cpp
	src/labenski/src/sheet.cpp
	src/labenski/src/sheetedg.cpp
	src/labenski/src/sheetedt.cpp
	src/labenski/src/sheetren.cpp
	src/labenski/src/sheetsel.cpp
	src/labenski/src/sheetspt.cpp
	src/labenski/src/sheettbl.cpp
	src/labenski/src/block.cpp
	src/labenski/src/genergdi.cpp
	src/labenski/src/geometry.cpp
	src/labenski/src/menubtn.cpp
	src/labenski/src/optvalue.cpp
	src/labenski/src/range.cpp
	src/labenski/src/spinctld.cpp
	src/labenski/src/toggle.cpp
	src/labenski/src/fparser.cpp
	src/labenski/src/fparser.hh
	src/labenski/src/fourier.c
	src/labenski/src/lm_lsqr.cpp
	src/labenski/src/plotctrl.cpp
	src/labenski/src/plotcurv.cpp
	src/labenski/src/plotdata.cpp
	src/labenski/src/plotdraw.cpp
	src/labenski/src/plotfunc.cpp
	src/labenski/src/plotmark.cpp
	src/labenski/src/plotprnt.cpp
	src/labenski/include/wx/plotctrl/fourier.h
	src/labenski/include/wx/plotctrl/fparser.h
	src/labenski/include/wx/plotctrl/lm_lsqr.h
	src/labenski/include/wx/plotctrl/plotctrl.h
	src/labenski/include/wx/plotctrl/plotcurv.h
	src/labenski/include/wx/plotctrl/plotdata.h
	src/labenski/include/wx/plotctrl/plotdefs.h
	src/labenski/include/wx/plotctrl/plotdraw.h
	src/labenski/include/wx/plotctrl/plotfunc.h
	src/labenski/include/wx/plotctrl/plotmark.h
	src/labenski/include/wx/plotctrl/plotprnt.h
	src/labenski/include/wx/sheet/pairarr.h
	src/labenski/include/wx/sheet/sheetatr.h
	src/labenski/include/wx/sheet/sheetdef.h
	src/labenski/include/wx/sheet/sheetedg.h
	src/labenski/include/wx/sheet/sheetedt.h
	src/labenski/include/wx/sheet/sheet.h
	src/labenski/include/wx/sheet/sheetren.h
	src/labenski/include/wx/sheet/sheetsel.h
	src/labenski/include/wx/sheet/sheetspt.h
	src/labenski/include/wx/sheet/sheettbl.h
	src/labenski/include/wx/sheet/wx24defs.h
	src/labenski/include/wx/wxthings/block.h
	src/labenski/include/wx/wxthings/bmpcombo.h
	src/labenski/include/wx/wxthings/dropdown.h
	src/labenski/include/wx/wxthings/genergdi.h
	src/labenski/include/wx/wxthings/geometry.h
	src/labenski/include/wx/wxthings/medsort.h
	src/labenski/include/wx/wxthings/menubtn.h
	src/labenski/include/wx/wxthings/optvalue.h
	src/labenski/include/wx/wxthings/range.h
	src/labenski/include/wx/wxthings/spinctld.h
	src/labenski/include/wx/wxthings/thingdef.h
	src/labenski/include/wx/wxthings/toggle.h
	src/gui/analysis.cc
	src/gui/analysis.h
	src/gui/edittext.cc
	src/gui/edittext.h
	src/gui/dcsvg.cc
	src/gui/dcsvg.h
	src/gui/dlabout.cc
	src/gui/dlabout.h
	src/gui/dleditmove.cc
	src/gui/dleditmove.h
	src/gui/dleditnode.cc
	src/gui/dleditnode.h
	src/gui/dlefglayout.cc
	src/gui/dlefglayout.h
	src/gui/dlefglegend.cc
	src/gui/dlefglegend.h
	src/gui/dlefglogit.cc
	src/gui/dlefglogit.h
	src/gui/dlefgreveal.cc
	src/gui/dlefgreveal.h
	src/gui/dlexcept.h
	src/gui/dlgameprop.cc
	src/gui/dlgameprop.h
	src/gui/dlinsertmove.cc
	src/gui/dlinsertmove.h
	src/gui/dlnash.cc
	src/gui/dlnash.h
	src/gui/dlnashmon.cc
	src/gui/dlnashmon.h
	src/gui/dlnfglogit.cc
	src/gui/efgdisplay.cc
	src/gui/efgdisplay.h
	src/gui/efglayout.cc
	src/gui/efglayout.h
	src/gui/efgpanel.cc
	src/gui/efgpanel.h
	src/gui/efgprofile.cc
	src/gui/efgprofile.h
	src/gui/gambit.cc
	src/gui/gambit.h
	src/gui/gamedoc.cc
	src/gui/gamedoc.h
	src/gui/gameframe.cc
	src/gui/gameframe.h
	src/gui/menuconst.h
	src/gui/nfgpanel.cc
	src/gui/nfgpanel.h
	src/gui/nfgprofile.cc
	src/gui/nfgprofile.h
	src/gui/nfgtable.cc
	src/gui/nfgtable.h
	src/gui/renratio.cc
	src/gui/renratio.h
	src/gui/style.cc
	src/gui/style.h
	src/gui/valnumber.cc
	src/gui/valnumber.h
	src/gui/tinyxml.cc
	src/gui/tinyxml.h
	src/gui/tinyxmlerror.cc
	src/gui/tinyxmlparser.cc
)

if(NOT Gambit_DISABLE_GUI)
	find_package(wxWidgets COMPONENTS core base adv)
	if(wxWidgets_FOUND)
		# TODO: Check wxWidgets version
		include(${wxWidgets_USE_FILE})
		add_executable(gambit ${gambit_SOURCES})
		target_link_libraries(gambit libgambit ${wxWidgets_LIBRARIES})
	else()
		message("wxWidgets not found. The GUI will be unavailable.")
	endif()
endif()


# Install rules
set(Gambit_INSTALL_TARGETS
	gambit
	gambit-enummixed
	gambit-enumpure
	gambit-gnm
	gambit-ipa
	gambit-lcp
	gambit-liap
	gambit-logit
	gambit-lp
	gambit-nfg-logitdyn
	gambit-nfg2html
	gambit-nfg2tex
	gambit-simpdiv
	gambit-testagg
)

if(NOT Gambit_DISABLE_ENUMPOLY)
	list(APPEND Gambit_INSTALL_TARGETS gambit-enumpoly)
endif()

install(TARGETS ${Gambit_INSTALL_TARGETS}
	RUNTIME DESTINATION bin
)

# Header files
install(FILES ${libagginclude_HEADERS} DESTINATION include/libagg)
install(FILES ${libgambitinclude_HEADERS} DESTINATION include/libgambit)


# Packaging
set(CPACK_GENERATOR "Bundle")
set(CPACK_PACKAGE_FILE_NAME "Gambit")
set(CPACK_BUNDLE_NAME "Gambit")
set(CPACK_BUNDLE_ICON ${PROJECT_SOURCE_DIR}/contrib/mac/gambit.icns)
set(CPACK_BUNDLE_PLIST ${PROJECT_SOURCE_DIR}/contrib/mac/Info.plist)

# Copy the script to launch the executable in the bundle.
set(CPACK_BUNDLE_STARTUP_COMMAND ${PROJECT_SOURCE_DIR}/contrib/mac/bundle-launch.sh)

include(CPack)
