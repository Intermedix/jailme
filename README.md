jailme - a setuid version of jexec to allow normal users access to FreeBSD jails

nkiraly/jailme/master Build Status: [![nkiraly/jailme/master Build Status](https://travis-ci.org/nkiraly/jailme.png?branch=master)](https://travis-ci.org/nkiraly/jailme)


# Origins

The jail system on FreeBSD allows convenient lightweight virtual machines within the FreeBSD operating system.

Some infrastructure deployments have their jails tightly controlled and monitored. Yet they have a need for non-administrative (i.e. application developers) to be able to access the jails - so that the infrastructure team doesn't have to hand-hold application upgrades, etc.

If a deployment's network infrastructure prevents you from allowing ssh access directly to the jails, there needs to be a way to shell into a given jail to do administraion and configuration. The [jexec(8)] (https://www.freebsd.org/cgi/man.cgi?query=jexec&apropos=0&sektion=8&manpath=FreeBSD+8.4-RELEASE&arch=default&format=html) program looks like a good solution - execpt it requires root privilidges to run - thus causing a requirement for too much access via sudo. Even with the fine-grained control that sudo gives you, the stock jexec causes the user to become root within the jail, and we don't want that either.


# How is jailme different?

jailme is a modified version of jexec. It works by being setuid, so that it can execute the jail_attach() call as root. It then tests the user name and user ID of the calling user to ensure that they are identical inside the jail to the host system. This acts as a sanity check. If the sanity check is successful, the user is assigned the appropriate credentials within the jail.

Because of the UID mapping requirement, I doubt this program will be useful to all jail systems, but I expect that there will be a subset of system configurations that find this program useful for keeping security tight. In theory, there's no reason it couldn't be part of the base system of FreeBSD, as it should not present any security problems on systems not planned to use it.


# Installation

There is a FreeBSD port you can install: [sysutils/jailme] (http://www.freshports.org/sysutils/jailme/)

Or build it yourself as root,

```bash
$ make install
```


# Usage

Usage is identical to [jexec(8)] (https://www.freebsd.org/cgi/man.cgi?query=jexec&apropos=0&sektion=8&manpath=FreeBSD+8.4-RELEASE&arch=default&format=html).

```bash
$ jailme jid command [...]
```


# Change log

## 0.1.0 (2014-10-28)

Documentation:
 - Update README, build tools, and packaging for migration from CFI People http://people.collaborativefusion.com/wmoran/code/jailme.html to Intermedix Github https://github.com/Intermedix/jailme

## 0.1 (2007-03-01)

Documentation:
 - Added text on possible security implications.

## 0.1 (2007-01-03)

Availability:
 - This is now part of the FreeBSD ports collection. Check out [sysutils/jailme] (http://www.freshports.org/sysutils/jailme/).

## 0.1 (2007-01-02)

Availability:
 - Submitted as a port to FreeBSD [ports/107441] (https://bugs.freebsd.org/bugzilla/show_bug.cgi?id=107441)

## 0.1 (2006-12-29)

Availability:
 - Initial public release 


# Security Concerns

If your system is not designed to use jailme, you could be creating security issues by installing it.

For example, if you have unpriviledged users on the host operating system, and accidentally create a user within a jail that had the same username and UID as the host system user, you could create an unintentional security breach. A specific example is that jailme does not check to see if the user has the same group membership in both the jail and the host system, so jailme may allow the user more access than intended by giving them more group privilidges within the jail.

I don't believe the risk of such a thing happening is very high. Also, system administrators who are aware of these security concerns can easily audit their systems and ensure they have not created such a situation.

If you believe you know of any way in which jailme presents a security issue, please contact me.
