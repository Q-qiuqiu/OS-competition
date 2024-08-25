/*
Copyright The Fission Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// Code generated by lister-gen. DO NOT EDIT.

package v1

import (
	v1 "github.com/fission/fission/pkg/apis/core/v1"
	"k8s.io/apimachinery/pkg/api/errors"
	"k8s.io/apimachinery/pkg/labels"
	"k8s.io/client-go/tools/cache"
)

// HTTPTriggerLister helps list HTTPTriggers.
// All objects returned here must be treated as read-only.
type HTTPTriggerLister interface {
	// List lists all HTTPTriggers in the indexer.
	// Objects returned here must be treated as read-only.
	List(selector labels.Selector) (ret []*v1.HTTPTrigger, err error)
	// HTTPTriggers returns an object that can list and get HTTPTriggers.
	HTTPTriggers(namespace string) HTTPTriggerNamespaceLister
	HTTPTriggerListerExpansion
}

// hTTPTriggerLister implements the HTTPTriggerLister interface.
type hTTPTriggerLister struct {
	indexer cache.Indexer
}

// NewHTTPTriggerLister returns a new HTTPTriggerLister.
func NewHTTPTriggerLister(indexer cache.Indexer) HTTPTriggerLister {
	return &hTTPTriggerLister{indexer: indexer}
}

// List lists all HTTPTriggers in the indexer.
func (s *hTTPTriggerLister) List(selector labels.Selector) (ret []*v1.HTTPTrigger, err error) {
	err = cache.ListAll(s.indexer, selector, func(m interface{}) {
		ret = append(ret, m.(*v1.HTTPTrigger))
	})
	return ret, err
}

// HTTPTriggers returns an object that can list and get HTTPTriggers.
func (s *hTTPTriggerLister) HTTPTriggers(namespace string) HTTPTriggerNamespaceLister {
	return hTTPTriggerNamespaceLister{indexer: s.indexer, namespace: namespace}
}

// HTTPTriggerNamespaceLister helps list and get HTTPTriggers.
// All objects returned here must be treated as read-only.
type HTTPTriggerNamespaceLister interface {
	// List lists all HTTPTriggers in the indexer for a given namespace.
	// Objects returned here must be treated as read-only.
	List(selector labels.Selector) (ret []*v1.HTTPTrigger, err error)
	// Get retrieves the HTTPTrigger from the indexer for a given namespace and name.
	// Objects returned here must be treated as read-only.
	Get(name string) (*v1.HTTPTrigger, error)
	HTTPTriggerNamespaceListerExpansion
}

// hTTPTriggerNamespaceLister implements the HTTPTriggerNamespaceLister
// interface.
type hTTPTriggerNamespaceLister struct {
	indexer   cache.Indexer
	namespace string
}

// List lists all HTTPTriggers in the indexer for a given namespace.
func (s hTTPTriggerNamespaceLister) List(selector labels.Selector) (ret []*v1.HTTPTrigger, err error) {
	err = cache.ListAllByNamespace(s.indexer, s.namespace, selector, func(m interface{}) {
		ret = append(ret, m.(*v1.HTTPTrigger))
	})
	return ret, err
}

// Get retrieves the HTTPTrigger from the indexer for a given namespace and name.
func (s hTTPTriggerNamespaceLister) Get(name string) (*v1.HTTPTrigger, error) {
	obj, exists, err := s.indexer.GetByKey(s.namespace + "/" + name)
	if err != nil {
		return nil, err
	}
	if !exists {
		return nil, errors.NewNotFound(v1.Resource("httptrigger"), name)
	}
	return obj.(*v1.HTTPTrigger), nil
}
