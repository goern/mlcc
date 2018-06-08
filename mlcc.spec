Name:    mlcc
Version: 0.1.0
Release: 1%{?dist}
Summary: A Machine Learning Stack Container Creator

License: GPLv2+
Source0: mlcc
BuildRequires: make, gcc

%description
Machine Learning Stack Container Creator

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
