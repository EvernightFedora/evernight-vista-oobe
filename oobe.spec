%global qt6_minver 6.6.0
%global kf6_minver 6.5.0


%global fullname plasma-setup
%global orgname org.kde.plasmasetup

# https://docs.fedoraproject.org/en-US/packaging-guidelines/#_compiler_flags
%global _hardened_build 1

Name:           evernight-vista-oobe
Version:        43.0
Release:        5%{?dist}
Summary:        Evernight Vista OOBE Components
License:        GPL-2.0-or-later
URL:            https://invent.kde.org/plasma/plasma-setup
Source:         https://github.com/EvernightFedora/evernight-vista-oobe/archive/refs/heads/main.zip
BuildRequires:  cmake(Qt6Core) >= %{qt6_minver}
BuildRequires:  cmake(Qt6Gui) >= %{qt6_minver}
BuildRequires:  cmake(Qt6Qml) >= %{qt6_minver}
BuildRequires:  cmake(Qt6QuickControls2) >= %{qt6_minver}
BuildRequires:  cmake(Qt6Svg) >= %{qt6_minver}
BuildRequires:  cmake(Qt6Widgets) >= %{qt6_minver}
BuildRequires:  cmake(Qt6DBus) >= %{qt6_minver}
BuildRequires:  cmake(KF6I18n) >= %{kf6_minver}
BuildRequires:  cmake(KF6Package) >= %{kf6_minver}
BuildRequires:  cmake(KF6Auth) >= %{kf6_minver}
BuildRequires:  cmake(KF6CoreAddons) >= %{kf6_minver}
BuildRequires:  cmake(KF6Config) >= %{kf6_minver}
BuildRequires:  cmake(KF6Screen)
BuildRequires:  cmake(LibKWorkspace)
BuildRequires:  cracklib-devel
BuildRequires:  extra-cmake-modules >= %{kf6_minver}
BuildRequires:  gcc-c++
BuildRequires:  systemd-rpm-macros
BuildRequires:  kf6-rpm-macros
BuildRequires:  libappstream-glib
BuildRequires:  qt6qml(org.kde.plasma.private.kcm_keyboard)

Requires:       qt6qml(org.kde.plasma.private.kcm_keyboard)
Conflicts:      kiss
Conflicts:      plasma-setup
Requires:       evernight-vista-wallpaper
Requires:       evernight-vista-themeui
Provides:       group(plasma-setup)
Provides:       group(kde-initial-system-setup)
Provides:       user(kde-initial-system-setup) = dSBrZGUtaW5pdGlhbC1zeXN0ZW0tc2V0dXAgLSAiS0RFIEluaXRpYWwgU3lzdGVtIFNldHVwIiAvcnVuL2tkZS1pbml0aWFsLXN5c3RlbS1zZXR1cCAvYmluL2Jhc2gA
Provides:       kiss >= 0.1.0~20250906git69c6007-2.fc43
Provides:       user(plasma-setup) = dSBwbGFzbWEtc2V0dXAgLSAiUGxhc21hIFNldHVwIiAvcnVuL3BsYXNtYS1zZXR1cCAvYmluL2Jhc2gA
Provides:       kiss(x86-64) >= 0.1.0~20250906git69c6007-2.fc43

# https://fedoraproject.org/wiki/Changes/EncourageI686LeafRemoval
ExcludeArch:    %{ix86}

# Do not check .so files in an application-specific library directory
%global __provides_exclude_from ^%{_kf6_qmldir}/org/kde/plasmasetup/.*\\.so.*$


%description
%{summary}.


%prep
%autosetup -n evernight-vista-oobe-main


%build
%cmake_kf6
%cmake_build


%install
%cmake_install

%find_lang %{orgname}
rm -fv %{buildroot}%{_kf6_libdir}/libcomponentspluginplugin.a




%files -f %{orgname}.lang
%license LICENSES/*
%{_libexecdir}/%{fullname}*
%{_kf6_libexecdir}/kauth/%{fullname}*
%{_kf6_qmldir}/org/kde/plasmasetup/
%{_kf6_plugindir}/packagestructure/plasmasetup.so
%{_kf6_datadir}/plasma/packages/org.kde.plasmasetup.*/
%{_unitdir}/%{fullname}*
%{_sysusersdir}/%{fullname}*
%{_tmpfilesdir}/%{fullname}*
%{_datadir}/dbus-1/*/%{orgname}.*
%{_datadir}/polkit-1/actions/%{orgname}.*
%{_datadir}/polkit-1/rules.d/%{fullname}*
%{_datadir}/qlogging-categories6/plasmasetup.categories
%{_datadir}/%{fullname}/


%changelog
* Sun Sep 07 2025 Neal Gompa <ngompa@fedoraproject.org> - 0.1.0~20250906git69c6007-2
- Drop i686 support as required dependencies are no longer available

* Sat Sep 06 2025 Neal Gompa <ngompa@fedoraproject.org> - 0.1.0~20250906git69c6007-1
- Bump to new git snapshot

* Thu Jul 24 2025 Fedora Release Engineering <releng@fedoraproject.org> - 0.1.0~20250524gitade7962-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_43_Mass_Rebuild

* Mon May 26 2025 Neal Gompa <ngompa@fedoraproject.org> - 0.1.0~20250524gitade7962-1
- Rebase to new rewrite

* Fri Jan 17 2025 Fedora Release Engineering <releng@fedoraproject.org> - 0~20211207git22cf331-10
- Rebuilt for https://fedoraproject.org/wiki/Fedora_42_Mass_Rebuild

* Mon Jul 29 2024 Miroslav Suchý <msuchy@redhat.com> - 0~20211207git22cf331-9
- convert license to SPDX

* Thu Jul 18 2024 Fedora Release Engineering <releng@fedoraproject.org> - 0~20211207git22cf331-8
- Rebuilt for https://fedoraproject.org/wiki/Fedora_41_Mass_Rebuild

* Thu Jan 25 2024 Fedora Release Engineering <releng@fedoraproject.org> - 0~20211207git22cf331-7
- Rebuilt for https://fedoraproject.org/wiki/Fedora_40_Mass_Rebuild

* Sun Jan 21 2024 Fedora Release Engineering <releng@fedoraproject.org> - 0~20211207git22cf331-6
- Rebuilt for https://fedoraproject.org/wiki/Fedora_40_Mass_Rebuild

* Thu Jul 20 2023 Fedora Release Engineering <releng@fedoraproject.org> - 0~20211207git22cf331-5
- Rebuilt for https://fedoraproject.org/wiki/Fedora_39_Mass_Rebuild

* Thu Jan 19 2023 Fedora Release Engineering <releng@fedoraproject.org> - 0~20211207git22cf331-4
- Rebuilt for https://fedoraproject.org/wiki/Fedora_38_Mass_Rebuild

* Thu Jul 21 2022 Fedora Release Engineering <releng@fedoraproject.org> - 0~20211207git22cf331-3
- Rebuilt for https://fedoraproject.org/wiki/Fedora_37_Mass_Rebuild

* Thu Jan 20 2022 Fedora Release Engineering <releng@fedoraproject.org> - 0~20211207git22cf331-2
- Rebuilt for https://fedoraproject.org/wiki/Fedora_36_Mass_Rebuild

* Tue Dec 07 2021 Marc Deop marcdeop@fedoraproject.org - 0~20211207git22cf331-1
- Initial Release

