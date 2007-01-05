/*
Copyright (c) 2003-2006, Troy Aaron Johnson
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of Troy Aaron Johnson nor the names of any
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

/** Iterates through nodes that have the same parent and allows changes.
 */
class sibling_iterator : public virtual iterator
{
  friend class tree;

  private:

    explicit sibling_iterator(tree_node<N>* current) : iterator(current) { }

  public:

    sibling_iterator(void) { }

    sibling_iterator(const iterator& iter) : iterator(iter) { }

    sibling_iterator& operator ++ (void)
    {
      sib_inc(this->current);
      return *this;
    }

    sibling_iterator operator ++ (int)
    {
      sibling_iterator tmp(*this);
      sib_inc(this->current);
      return tmp;
    }

    sibling_iterator& operator -- (void)
    {
      sib_dec(this->current);
      return *this;
    }

    sibling_iterator operator -- (int)
    {
      sibling_iterator tmp(*this);
      sib_dec(this->current);
      return tmp;
    }

    sibling_iterator operator + (unsigned int n) const
    {
      sibling_iterator i(*this);

      while (n-- > 0)
        ++i;

      return i;
    }

    sibling_iterator& operator += (unsigned int n)
    {
      while (n-- > 0)
        sib_inc(this->current);

      return *this;
    }

    sibling_iterator operator - (unsigned int n) const
    {
      sibling_iterator i(*this);

      while (n-- > 0)
        --i;

      return i;
    }

    sibling_iterator& operator -= (unsigned int n)
    {
      while (n-- > 0)
        sib_dec(this->current);

      return *this;
    }
};
