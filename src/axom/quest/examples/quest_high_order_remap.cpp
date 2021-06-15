// Copyright (c) 2017-2019, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

/*!
 * \file quest_high_order_remap.cpp
 * \brief Demonstrates conservative field remap on 2D high order meshes
 */

#include <chrono>

// Axom includes
#include "axom/core.hpp"
#include "axom/slic.hpp"
#include "axom/primal.hpp"
#include "axom/spin.hpp"

#ifdef AXOM_USE_MFEM
  #include "mfem.hpp"
#else
  #error "This example requires mfem"
#endif

#include "fmt/fmt.hpp"

#include <fstream>

//namespace mint = axom::mint;
namespace primal = axom::primal;
namespace spin = axom::spin;

/**
 * \brief Wrapper for a 2D mfem mesh
 *
 * Helps with conversion to BezierCurves and CurvedPolygons
 *
 * The meshwrapper assumes ownership of the wrapped mesh
 */
class MeshWrapper
{
public:
  using BBox = primal::BoundingBox<double, 2>;
  using Point = primal::Point<double, 2>;

  using CurvedPolygonType = primal::CurvedPolygon<double, 2>;
  using BCurve = CurvedPolygonType::BezierCurveType;

private:
  /*! \brief Checks if the mesh's nodes are in the Bernstein basis */
  bool isBernsteinBasis() const
  {
    auto* fec = m_mesh->GetNodalFESpace()->FEColl();

    if(fec == nullptr)
    {
      return false;
    }

    if(const mfem::H1_FECollection* h1Fec =
         dynamic_cast<const mfem::H1_FECollection*>(fec))
    {
      return h1Fec->GetBasisType() == mfem::BasisType::Positive;
    }

    if(const mfem::L2_FECollection* l2Fec =
         dynamic_cast<const mfem::L2_FECollection*>(fec))
    {
      return l2Fec->GetBasisType() == mfem::BasisType::Positive;
    }

    if(dynamic_cast<const mfem::NURBSFECollection*>(fec) ||
       dynamic_cast<const mfem::LinearFECollection*>(fec) ||
       dynamic_cast<const mfem::QuadraticPosFECollection*>(fec))
    {
      return true;
    }

    return false;
  }

public:
  MeshWrapper() : m_mesh(nullptr) { }

  MeshWrapper(mfem::Mesh* mesh) { setMesh(mesh); }

  ~MeshWrapper()
  {
    if(m_mesh != nullptr)
    {
      delete m_mesh;
      m_mesh = nullptr;
    }
  }

  /*!
   * Sets the mfem mesh pointer for this MeshWrapper instance
   */
  void setMesh(mfem::Mesh* mesh)
  {
    SLIC_ASSERT(mesh != nullptr);
    m_mesh = mesh;

    bool isHighOrder =
      (m_mesh->GetNodalFESpace() != nullptr) && (m_mesh->GetNE() > 0);
    SLIC_ASSERT_MSG(isHighOrder, "The mesh must be high order.");

    bool isBernstein = isBernsteinBasis();
    SLIC_ASSERT_MSG(isBernstein, "The mesh must be in the Bernstein basis");

    const double tol = 1E-8;
    computeBoundingBoxes(1 + tol);
  }

  int numVertices() const { return m_mesh->GetNV(); }
  int numElements() const { return m_mesh->GetNE(); }

  bool hasMesh() const { return m_mesh != nullptr; }
  mfem::Mesh* getMesh() { return m_mesh; }
  const mfem::Mesh* getMesh() const { return m_mesh; }

  const BBox& elementBoundingBox(int i) const { return m_boundingBoxes[i]; }
  const BBox& meshBoundingBox() const { return m_meshBBox; }

  /*!
   * \brief Transform the mfem element into a primal CurvedPolygon
   *
   * \param elemId The index of the element
   * \return The element as a CurvedPolygon composed of BezierCurves
   */
  CurvedPolygonType elemAsCurvedPolygon(int elemId)
  {
    SLIC_ASSERT(elemId >= 0 && elemId < numElements());

    auto* fes = m_mesh->GetNodalFESpace();
    auto* nodes = m_mesh->GetNodes();

    // Get the edge Ids for this element
    mfem::Array<int> edgeIds, edgeOrients;
    m_mesh->GetElementEdges(elemId, edgeIds, edgeOrients);
    const int nEdges = edgeIds.Size();

    CurvedPolygonType poly(nEdges);
    const int order = fes->GetOrder(0);

    // Get the grid function data associated with this edge
    mfem::Array<int> dofIndices;
    BCurve curve(order);
    for(int e = 0; e < nEdges; ++e)
    {
      // get the dof (degree of freedom) indices for this edge
      fes->GetEdgeDofs(edgeIds[e], dofIndices);

      //SLIC_INFO("Elem " << elemId
      // << " edge " << edgeIds[e] << " w/ orient " << edgeOrients[e]
      // << " -- dof inds "
      // << dofIndices[0] << " " << dofIndices[1] << " "  << dofIndices[2]
      // << " -- points "
      // << spacePointFromDof(dofIndices[0], fes, nodes) << " "
      // << spacePointFromDof(dofIndices[1], fes, nodes) << " "
      // << spacePointFromDof(dofIndices[2], fes, nodes)
      // );

      // possibly reverse the dofs, based on the orientation
      // Note: The dofs are ordered by vertices, then by edge
      const bool bReverse = (edgeOrients[e] > 0);
      if(bReverse)
      {
        curve[0] = spacePointFromDof(dofIndices[1], fes, nodes);
        for(int p = 1; p < order; ++p)
        {
          curve[p] = spacePointFromDof(dofIndices[order - (p - 1)], fes, nodes);
        }
        curve[order] = spacePointFromDof(dofIndices[0], fes, nodes);
      }
      else
      {
        curve[0] = spacePointFromDof(dofIndices[0], fes, nodes);
        for(int p = 1; p < order; ++p)
        {
          curve[p] = spacePointFromDof(dofIndices[p + 1], fes, nodes);
        }
        curve[order] = spacePointFromDof(dofIndices[1], fes, nodes);
      }

      // Note: mfem's orientation is reversed w.r.t. primal's
      //SLIC_INFO("Elem " << elemId << " edge " << e << " -- curve: " << curve);
      //curve.reverseOrientation();
      poly[nEdges - e - 1] = curve;
    }
    //  std::cout << poly << std::endl;
    return poly;
  }

private:
  /*! Get the coordinates of the point from the dof index */
  Point spacePointFromDof(int idx,
                          const mfem::FiniteElementSpace* fes,
                          const mfem::GridFunction* nodes)
  {
    return Point::make_point((*nodes)(fes->DofToVDof(idx, 0)),
                             (*nodes)(fes->DofToVDof(idx, 1)));
  }

  /*!
   * \brief Compute the element and mesh bounding boxes
   *
   * \param[in] bboxScaleFac Scale factor to increase the bounding boxes
   * \pre bboxScaleFac >= 1.
   */
  void computeBoundingBoxes(double bboxScaleFac)
  {
    SLIC_ASSERT(bboxScaleFac >= 1.);

    m_boundingBoxes.resize(numElements());
    m_meshBBox.clear();

    auto* nodes = m_mesh->GetNodes();
    auto* fes = m_mesh->GetNodalFESpace();
    mfem::Array<int> dofIndices;

    const int NE = numElements();
    for(int elem = 0; elem < NE; ++elem)
    {
      auto& bbox = m_boundingBoxes[elem];
      bbox.clear();

      // Add each dof of the element to the bbox
      // Note: positivity of Bernstein bases ensures that convex
      //       hull of element nodes contain entire element
      fes->GetElementDofs(elem, dofIndices);
      for(int i = 0; i < dofIndices.Size(); ++i)
      {
        int nIdx = dofIndices[i];
        bbox.addPoint(spacePointFromDof(nIdx, fes, nodes));
      }

      // Slightly scale the bbox to account for numerical noise
      bbox.scale(bboxScaleFac);

      m_meshBBox.addBox(bbox);
    }
  }

private:
  mfem::Mesh* m_mesh;

  std::vector<BBox> m_boundingBoxes;
  BBox m_meshBBox;
};

struct Remapper
{
private:
  using GridType = spin::ImplicitGrid<2, int>;

public:
  using CandidateList = std::vector<int>;

public:
  Remapper() = default;

  ~Remapper()
  {
    fecMap.DeleteData(true);
    fesMap.DeleteData(true);
  }

  /*! Set up the source and target meshes */
  void setupMeshes(int res1, int res2, int order)
  {
    const auto quadType = mfem::Element::QUADRILATERAL;
    const int dim = 2;

    // paramters for source  mesh -- quad mesh covering unit square
    const int src_res = res2;
    const int src_ord = order;

    // paramters for target mesh -- quad mesh covering (part of) unit square
    const int tgt_res = res1;
    const int tgt_ord = order;
    const double tgt_scale = .712378102150;
    const double tgt_trans = .1345747181586;

    // create the source mesh
    {
      // create mfem mesh
      auto* mesh = new mfem::Mesh(src_res, src_res, quadType, true);

      // create finite element collection for nodes
      auto* fec =
        new mfem::H1_FECollection(src_ord, dim, mfem::BasisType::Positive);
      fecMap.Register("src_fec", fec, true);

      // create finite element space for nodes
      auto* fes = new mfem::FiniteElementSpace(mesh, fec, dim);
      fesMap.Register("src_fes", fes, true);
      mesh->SetNodalFESpace(fes);

      // SLIC_INFO("Writing to: " << axom::utilities::filesystem::getCWD());
      {
        std::ofstream file;
        file.open("source_mesh.mfem");
        mesh->Print(file);
      }

      srcMesh.setMesh(mesh);
    }

    // create the target mesh
    {
      auto* mesh = new mfem::Mesh(tgt_res, tgt_res, quadType, true);
      xformMesh(mesh, tgt_scale, tgt_trans);

      auto* fec =
        new mfem::H1_FECollection(tgt_ord, dim, mfem::BasisType::Positive);
      fecMap.Register("tgt_fec", fec, true);

      auto* fes = new mfem::FiniteElementSpace(mesh, fec, dim);
      fesMap.Register("tgt_fes", fes, true);
      mesh->SetNodalFESpace(fes);

      {
        std::ofstream file;
        file.open("target_mesh.mfem");
        mesh->Print(file);
      }

      tgtMesh.setMesh(mesh);
    }
  }
  void loadMeshes(int res2, int order)
  {
    // create the source mesh
    const auto quadType = mfem::Element::QUADRILATERAL;
    const int dim = 2;

    // paramters for target mesh -- quad mesh covering unit square
    const int src_res = res2;
    const int src_ord = order;
    const double src_scale = 6.0;
    const double src_trans = -3.001;

    // paramters for target mesh -- quad mesh covering (part of) unit square
    const int tgt_ord = 2;

    {
      // create mfem mesh
      auto* mesh = new mfem::Mesh(src_res, src_res, quadType, true);
      xformMesh(mesh, src_scale, src_trans);

      // create finite element collection for nodes
      auto* fec =
        new mfem::H1_FECollection(src_ord, dim, mfem::BasisType::Positive);
      fecMap.Register("src_fec", fec, true);

      // create finite element space for nodes
      auto* fes = new mfem::FiniteElementSpace(mesh, fec, dim);
      fesMap.Register("src_fes", fes, true);
      mesh->SetNodalFESpace(fes);

      // SLIC_INFO("Writing to: " << axom::utilities::filesystem::getCWD());
      {
        std::ofstream file;
        file.open("source_mesh.mfem");
        mesh->Print(file);
      }

      srcMesh.setMesh(mesh);
    }
    // create the target mesh
    {
      auto* mesh = new mfem::Mesh("./disc-nurbs-80.mesh", 1, 1);
      if(mesh->NURBSext)
      {
        int order = tgt_ord;
        mesh->SetCurvature(order);
      }
      // xformMesh(mesh, tgt_scale, tgt_trans);
      {
        std::ofstream file;
        file.open("target_mesh_orig.mfem");
        mesh->Print(file);
      }

      auto* fec =
        new mfem::H1_FECollection(tgt_ord, dim, mfem::BasisType::Positive);
      fecMap.Register("tgt_fec", fec, true);

      auto* fes = new mfem::FiniteElementSpace(mesh, fec, dim);
      fesMap.Register("tgt_fes", fes, true);
      mesh->SetNodalFESpace(fes);

      {
        std::ofstream file;
        file.open("target_mesh_set.mfem");
        mesh->Print(file);
      }
      std::cout << "Got here!" << std::endl;
      tgtMesh.setMesh(mesh);
    }
  }
  /*! Setup the implicit grid spatial index over the source mesh */
  void setupGrid()
  {
    const int NE = srcMesh.numElements();
    grid.initialize(srcMesh.meshBoundingBox(), nullptr, NE);

    for(int i = 0; i < NE; ++i)
    {
      grid.insert(srcMesh.elementBoundingBox(i), i);
    }
  }

  /*!
   * Computes the overlap areas between the elements of the target mesh
   * to the elements of the source mesh
   */
  double computeOverlapAreas()
  {
    double totalArea = 0.0;
    double correctArea = 0.0;
    const int nTargetElems = tgtMesh.numElements();
    //  SLIC_INFO("Number of Target Elements: " << nTargetElems);
    double calcE = 0.0;
    for(int i = 0; i < nTargetElems; ++i)
    {
      // Finds the candidates from the source mesh that
      // can intersect this target element
      auto candidates = getSourceCandidates(i);
      if(candidates.empty()) break;

      auto tgtPoly = tgtMesh.elemAsCurvedPolygon(i);
      //  SLIC_INFO("Target Element: " << tgtPoly);
      correctArea += tgtPoly.area();
      //j  SLIC_INFO("Target elem " << i
      //j                           << " -- area " << tgtPoly.area()
      //j            //<< " -- bbox " << tgtMesh.elementBoundingBox(i)
      //j            );

      double A = 0.0;
      for(int srcElem : candidates)
      {
        auto srcPoly = srcMesh.elemAsCurvedPolygon(srcElem);
        //   SLIC_INFO("*Source Element: " << srcPoly);
        //   SLIC_INFO("* Source elem " << srcElem
        //                              << " -- area " << srcPoly.area()
        ////             //<< " -- bbox " << srcMesh.elementBoundingBox(srcElem)
        //             );

        std::vector<primal::CurvedPolygon<double, 2>> pnew;
        tgtPoly.reverseOrientation();
        srcPoly.reverseOrientation();
        if(primal::intersect_polygon(tgtPoly, srcPoly, pnew))
        {
          for(int i = 0; i < static_cast<int>(pnew.size()); ++i)
          {
            A -= pnew[i].area();
            //   SLIC_INFO("** Intersection area :" << -pnew[i].area()
            //            );
          }
        }
        srcPoly.reverseOrientation();
        tgtPoly.reverseOrientation();
        //  SLIC_INFO("* Calculated area:  " << srcElem
        //                             << " -- area " << A
        //            //<< " -- bbox " << srcMesh.elementBoundingBox(srcElem)
        //            );
      }
      calcE += abs(tgtPoly.area() - A);
      totalArea += A;
      //  SLIC_INFO("Calculated Area :" << A);
    }
    //  std::cout << inclusion << ", ";
    //  std::cout << calcE << ", ";
    //  const double tgt_scale = .999712378102150;
    //  const double tgt_trans = .0001345747181586;
    //  const double tgt_scale = .9999712378102150;
    //  const double tgt_trans = .00001345747181586;
    //  const double tgt_scale = .99999999999712378102150;
    //  const double tgt_trans = .000000000001345747181586;
    //  double trueError = (tgt_scale*tgt_scale-totalArea);
    //  std::cout << trueError << ", ";
    std::cout << totalArea << std::endl;
    std::cout << correctArea << std::endl;
    return (totalArea - correctArea);
  }

  /*!
   * Gets the IDs of the candidate elements from the source mesh
   * that might intersect with \a targetElemID from the target mesh
   */
  CandidateList getSourceCandidates(int targetElemID) const
  {
    SLIC_ASSERT(targetElemID < tgtMesh.numElements());
    using BitsetType = GridType::BitsetType;

    CandidateList filteredCandidates;

    auto& targetBBox = tgtMesh.elementBoundingBox(targetElemID);
    auto candidateBits = grid.getCandidates(targetBBox);

    // Filter the candidates; return as std vec
    for(auto idx = candidateBits.find_first(); idx != BitsetType::npos;
        idx = candidateBits.find_next(idx))
    {
      if(primal::intersect(targetBBox, srcMesh.elementBoundingBox(idx)))
        filteredCandidates.push_back(idx);
    }

    return filteredCandidates;
  }

public:
  MeshWrapper srcMesh;
  MeshWrapper tgtMesh;
  GridType grid;

private:
  // Named fields map manage memory associated with the
  // finite element collection and spaces
  mfem::NamedFieldsMap<mfem::FiniteElementCollection> fecMap;
  mfem::NamedFieldsMap<mfem::FiniteElementSpace> fesMap;

private:
  // scale and translate the vertices of the given mesh
  void xformMesh(mfem::Mesh* mesh, double sc, double off)
  {
    std::cout << "Transforming Mesh now" << std::endl;
    for(int v = 0; v < mesh->GetNV(); ++v)
    {
      double* pt = mesh->GetVertex(v);
      //  if (v==0)
      //  {
      //    std::cout << pt[0] << " , " << pt[1] << std::endl;
      //  }
      //  pt[0] = sc*pt[0] + off;
      pt[0] = sc * pt[0] + off + .000147582957;
      pt[1] = sc * pt[1] + off;

      /* if (v%3==0)
      {
        pt[0] = pt[0]-.01748;
      }
      if (v%5==0)
      {
        pt[0] = pt[0]+.004;
      }
      if (v%7==0)
      {
        pt[1] = pt[1]+.0243;
      }*/
      //  if (v==0)
      //  {
      //    std::cout << pt[0] << " , " << pt[1] << std::endl;
      //  }
    }
    //   for (int e =0 ; e < mesh->GetNEdges() ; ++e)
    //   {
    //     mfem::array<int> dofs();
    //     GetEdgeDofs(e, dofs);
    //   }
  }
};

//------------------------------------------------------------------------------
int main(int argc, char** argv)
{
  axom::slic::SimpleLogger logger;

  SLIC_INFO("The application conservatively maps fields from a source \n"
            << "high order mesh to a target high order mesh!");
  //  int res1=1;
  //  Remapper remap;
  //  res1=5;
  //  int res2=res1+1;
  //  remap.loadMeshes(res1,2);
  //  remap.setupGrid();
  //  double Area = remap.computeOverlapAreas();
  //  std::cout << std::endl << Area << std::endl;

  //  return 0;

  int res1 = 1;
  std::cout << "[";
  for(int i = 2; i <= 2; ++i)
  {
    Remapper remap;

    // res1= res1*2;

    res1 = 25;
    int res2 = res1 + 2;
    // Setup the two meshes in the Bernstein basis
    // The current implementation hard-codes the two meshes
    // TODO: Read in two meshes from disk.
    //       In that case, we will need to convert the FEC to the Bernstein basis
    //  remap.setupMeshes(res1, res2, i);
    remap.loadMeshes(res1, i);

    // Set up the spatial index
    remap.setupGrid();
    auto start = std::chrono::high_resolution_clock::now();
    // Computes the overlaps between elements of the target and source meshes
    double Area = remap.computeOverlapAreas();
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << i << ", " << Area << "," << elapsed.count() << std::endl;
  }
  std::cout << "]" << std::endl;
  return 0;
}