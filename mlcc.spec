Name:    mlcc
Version: 0.1.0
Release: 1%{?dist}
Summary: A Machine Learning Stack Container Creator

%global commit d20b8c39f06d22a00301822d1cc829425645cf42
%global shortcommit %(c=%{commit}; echo ${c:0:7})

License: GPLv2+
Source0: https://github.com/goern/%{name}/archive/%{commit}/%{name}-%{shortcommit}.tar.gz
BuildRequires: make, gcc

%description
Machine Learning Stack Container Creator

%prep
%autosetup -n %{name}-%{commit}

%install
mkdir -p %{buildroot}/%{_bindir}
install -p -m 755 %{SOURCE0} %{buildroot}/%{_bindir}

%build  
make

%files
%{_bindir}/mlcc

%changelog
* Fri Jun 8 2018 goern 0.1.0-1
- Initial commit to build mlcc via copr
