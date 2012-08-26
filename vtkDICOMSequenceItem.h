#ifndef __vtkDICOMSequenceItem_h
#define __vtkDICOMSequenceItem_h

#include "vtkDICOMDataElement.h"

//! An item in a DICOM sequence (type SQ).
/*!
 *  A DICOM sequence is a list of items, where each item is
 *  essentially a data set of its own.  An item consists of
 *  zero or more data elements, each with a tag and value.
 */
class vtkDICOMSequenceItem
{
  //! A reference counted list container class.
  struct List
  {
    unsigned int ReferenceCount;
    int NumberOfDataElements;
    vtkDICOMDataElement Head;
    vtkDICOMDataElement Tail;

    List() : ReferenceCount(1), NumberOfDataElements(0) {
      this->Head.Prev = 0;
      this->Head.Next = &this->Tail;
      this->Tail.Prev = &this->Head;
      this->Tail.Next = 0; }

    ~List() {
      vtkDICOMDataElement *ptr = this->Head.Next;
      while (ptr != &this->Tail) {
        ptr = ptr->Next;
        delete ptr->Prev; } }
  };

public:
  vtkDICOMSequenceItem() : L(0) {}

  //! Copy constructor does reference counting.
  vtkDICOMSequenceItem(const vtkDICOMSequenceItem &o) : L(o.L) {
    if (this->L) { this->L->ReferenceCount++; } }

  //! Destructor does reference counting.
  ~vtkDICOMSequenceItem() { this->Clear(); }

  //! Clear the data.
  void Clear() {
    if (this->L && --this->L->ReferenceCount == 0) { delete this->L; }
    this->L = 0; }

  //! Check if empty.
  bool IsEmpty() const { return (this->L == 0); }

  //! Add a data element to this item.
  void SetAttributeValue(vtkDICOMTag tag, const vtkDICOMValue& v);

  //! Get a data element from this item.
  bool GetAttributeValue(vtkDICOMTag tag, vtkDICOMValue& v) const;

  //! Get the number of data elements.
  int GetNumberOfDataElements() const {
    return (this->L ? this->L->NumberOfDataElements : 0); }

  //! Get an iterator for the list of data elements.
  vtkDICOMDataElementIterator GetData() const {
    return (this->L ? this->L->Head.Next : 0); }

  //! Get an end iterator for the list of data elements.
  vtkDICOMDataElementIterator GetDataEnd() const {
    return (this->L ? &this->L->Tail : 0); }

  bool operator==(const vtkDICOMSequenceItem& o) const;
  bool operator!=(const vtkDICOMSequenceItem& o) const {
    return !(*this == o); }

  //! Assignment operator does reference counting.
  vtkDICOMSequenceItem &operator=(const vtkDICOMSequenceItem &o) {
    if (this->L != o.L) {
      if (o.L) { o.L->ReferenceCount++; }
      if (this->L && --this->L->ReferenceCount == 0) { delete this->L; }
      this->L = o.L; }
    return *this; }

private:
  void CopyList(const List *o, List *t);

  //! A linked list to hold the elements.
  List *L;
};

#endif /* __vtkDICOMSequenceItem_h */
