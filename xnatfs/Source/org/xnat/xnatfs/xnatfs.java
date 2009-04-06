/**
 *   FUSE-J: Java bindings for FUSE (Filesystem in Userspace by Miklos Szeredi (mszeredi@inf.bme.hu))
 *
 *   Copyright (C) 2003 Peter Levart (peter@select-tech.si)
 *
 *   This program can be distributed under the terms of the GNU LGPL.
 *   See the file COPYING.LIB
 */

package org.xnat.xnatfs;

import org.apache.log4j.*;
import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import java.io.File;
import java.nio.ByteBuffer;
import java.nio.BufferOverflowException;
import java.nio.CharBuffer;
import java.util.*;
import fuse.*;

@SuppressWarnings({"OctalInteger"})
public class xnatfs implements Filesystem3, LifecycleSupport {
  private static final String filename = "/HelloWorld.txt";
  private static final byte[] contents = "Hello world!\n".getBytes();
  private static final Logger logger = Logger.getLogger(xnatfs.class);
  
   private static final int BLOCK_SIZE = 512;
   private static final int NAME_LENGTH = 1024;

   public xnatfs() {
   }

   public int chmod(String path, int mode) throws FuseException {
     logger.info ( "chmod: " + path );
     if ( !path.equals ( filename ) ) {
       return Errno.ENOENT;
     }
     return 0;
   }
   public int chown(String path, int uid, int gid) throws FuseException {
     logger.info ( "chown: " + path );
      return 0;
   }

  public int getattr(String path, FuseGetattrSetter getattrSetter) throws FuseException {
    logger.info ( "getattr: " + path );
    int time = (int) (System.currentTimeMillis() / 1000L);
    if ( path.equals ( "/" ) ) {
         getattrSetter.set(
            this.hashCode(),
            FuseFtypeConstants.TYPE_DIR | 0755,
            3,
            0, 0,
            0,
            1 * NAME_LENGTH,
            (1 * NAME_LENGTH + BLOCK_SIZE - 1) / BLOCK_SIZE,
            time, time, time
         );
         return 0;
    }
    if ( path.equals ( filename ) ) {
      // set(long inode, int mode, int nlink, int uid, int gid, int rdev, long size, long blocks, int atime, int mtime, int ctime) 
      getattrSetter.set(
                        filename.hashCode(),
                        FuseFtypeConstants.TYPE_FILE | 0644,
                        1,
                        0, 0,
                        0,
                        contents.length,
                        (contents.length + BLOCK_SIZE - 1) / BLOCK_SIZE,
                        time, time, time
                        );
      return 0;
    }
    return Errno.ENOENT;
  }
  
  public int getdir(String path, FuseDirFiller filler) throws FuseException {
     logger.info ( "getdir: " + path );
    // Root path
    if ( path.equals ( "/" ) ) {
      int ftype = FuseFtypeConstants.TYPE_FILE;
      filler.add(
                 filename,
                 filename.hashCode(),
                 ftype | 0644
                 );
      return 0;
    }
    return Errno.ENOTDIR;
  }

   public int link(String from, String to) throws FuseException {
      return Errno.EROFS;
   }

   public int mkdir(String path, int mode) throws FuseException {
      return Errno.EROFS;
   }

   public int mknod(String path, int mode, int rdev) throws FuseException {
      return Errno.EROFS;
   }

   public int rename(String from, String to) throws FuseException {
      return Errno.EROFS;
   }

   public int rmdir(String path) throws FuseException {
      return Errno.EROFS;
   }

   public int statfs(FuseStatfsSetter statfsSetter) throws FuseException {
     logger.info ( "statfs" );
     // set(int blockSize, int blocks, int blocksFree, int blocksAvail, int files, int filesFree, int namelen) 
     statfsSetter.set ( BLOCK_SIZE,
                        1000,
                        200,
                        180,
                        1,
                        0,
                        NAME_LENGTH
                        );
     return 0;
   }

   public int symlink(String from, String to) throws FuseException {
      return Errno.EROFS;
   }

   public int truncate(String path, long size) throws FuseException {
      return Errno.EROFS;
   }

   public int unlink(String path) throws FuseException {
      return Errno.EROFS;
   }

   public int utime(String path, int atime, int mtime) throws FuseException {
      return 0;
   }

  // Read the correct name of a linked file
   public int readlink(String path, CharBuffer link) throws FuseException {
      return Errno.ENOENT;
   }

   // if open returns a filehandle by calling FuseOpenSetter.setFh() method, it will be passed to every method that supports 'fh' argument
   public int open(String path, int flags, FuseOpenSetter openSetter) throws FuseException {
     logger.info ( "open: " + path );
     // set the file handle object
     if ( path.equals ( filename ) ) {
       openSetter.setFh(filename);
       return 0;
     }
      return Errno.ENOENT;
   }

   // fh is filehandle passed from open,
   // isWritepage indicates that write was caused by a writepage
   public int write(String path, Object fh, boolean isWritepage, ByteBuffer buf, long offset) throws FuseException {
     logger.info ( "write: " + path );
      return Errno.EROFS;
   }

   // fh is filehandle passed from open
   public int read(String path, Object fh, ByteBuffer buf, long offset) throws FuseException {
     logger.info ( "read: " + path );
     if ( fh == filename ) {
         buf.put(contents, (int) offset, Math.min(buf.remaining(), contents.length - (int)offset));
         return 0;
      }
      return Errno.EBADF;
   }

   // new operation (called on every filehandle close), fh is filehandle passed from open
   public int flush(String path, Object fh) throws FuseException {
     logger.info ( "flush: " + path );
     if ( fh == filename ) {
         return 0;
     }
     return Errno.EBADF;
   }
  
   // new operation (Synchronize file contents), fh is filehandle passed from open,
   // isDatasync indicates that only the user data should be flushed, not the meta data
   public int fsync(String path, Object fh, boolean isDatasync) throws FuseException {
     logger.info ( "fsync: " + path );
     if ( fh == filename ) {
       return 0;
     }
     return Errno.EBADF;
   }

   // (called when last filehandle is closed), fh is filehandle passed from open
   public int release(String path, Object fh, int flags) throws FuseException {
     logger.info ( "release: " + path );
     if ( fh == filename ) {
       return 0;
     }
     return Errno.EBADF;
   }


    //
    // LifeCycleSupport
    public int init() {
        logger.info("Initializing Filesystem");
        return 0;
    }

    public int destroy() {
        logger.info("Destroying Filesystem");
        return 0;
    }

    //
    // Java entry point
  public static void main(String[] args) {
    BasicConfigurator.configure();
    logger.info("entering");
    
    try {
      Log l = LogFactory.getLog("org.xnat.xnatfs.FuseMount");
      FuseMount.mount(args, new xnatfs(), l);
    }
    catch(Exception e) {
      e.printStackTrace();
    }
    finally {
      logger.info("exiting");
    }
  }
}
